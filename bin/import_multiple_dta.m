function [d_hitbased d_timebased] = import_multiple_dta ( files )
%IMPORT_MULTIPLE_DTA codistributed array from .dta filename array
%   This function will return two codistributed arrays - one with
%   all of the hit-based data and one with all of the time-based
%   data.  This occurs in parallel.

n = length(files);
nlabs = matlabpool('size');
local_hitbased = [];
local_timebased = [];
local_m = [];
n_channels = 16;

n_files_to_process = -1;
if (mod (n, nlabs) ~= 0)
    n_files_to_process = ...
        ([1:nlabs] <= mod (n, nlabs)) .* ceil (n/nlabs) ...
        + ([1:nlabs] > mod (n, nlabs)) .* floor (n/nlabs);
else
    n_files_to_process = ones (1, nlabs) .* ceil (n/nlabs);
end

offsets = circshift (n_files_to_process, [0 1]);
offsets (1) = 0;

hb_dims = zeros(1,nlabs);
tb_dims = zeros(1,nlabs);

spmd
    for ind=1:n_files_to_process (labindex)
        [a, b, m] = import_dta( files {offsets (labindex) + ...
            ind} );
        if isempty (local_hitbased)
            local_hitbased = a;
            local_timebased = b;
            local_m = m;
        else
            for index=1:length (local_hitbased)
                fns = fieldnames (local_hitbased(index));
                for field=1:length (fns)
                    local_hitbased(index).(fns{field}) = ...
                        [local_hitbased(index).(fns{field}) ...
                        a(index).(fns{field})];
                end
            end
            local_timebased.tot = [local_timebased.tot b.tot];
            local_timebased.parametrics = ...
                [local_timebased.parametrics b.parametrics];
            for ch=1:length(b.ae_characteristics)
                fns = fieldnames (b.ae_characteristics(ch));
                for field=1:length (fns)
                    local_timebased.ae_characteristics(ch).(fns{field}) = ...
                        [local_timebased.ae_characteristics(ch).(fns{field}) ...
                        b.ae_characteristics(ch).(fns{field})];
                end
            end
        end
    end
    
    if (labindex == 1)
        n_channels_g = labBroadcast (1, size (local_hitbased,2));
        n_par_hb_g = labBroadcast (1, size (local_hitbased (1).parametrics, 1));
        n_par_tb_g = labBroadcast (1, size (local_timebased.parametrics, 1));
        ae_chars_hb_g = labBroadcast (1, fieldnames (local_hitbased (1)));
        ae_chars_tb_g = labBroadcast (1, fieldnames (local_timebased.ae_characteristics (1)));
        n_samples_l = zeros (1,n_channels_g);
        n_data_per_event_hb_l = zeros (n_channels_g, numel (ae_chars_hb_g));
        for ch=1:n_channels_g
            n_samples_l (ch) = labBroadcast (1, size (local_hitbased (ch).x, 1));
            for ae_ch=1:numel (ae_chars_hb_g)
                n_data_per_event_hb_l (ch, ae_ch) = ...
                    size (local_hitbased (ch).(ae_chars_hb_g {ae_ch}), 1);
            end
        end
        n_data_per_event_tb_l = zeros (1, numel ( ae_chars_hb_g));
        for ae_ch=1:numel (ae_chars_tb_g)
            n_data_per_event_tb_l(ae_ch) = ...
                size(local_timebased.ae_characteristics(ae_ch).(ae_chars_tb_g{ae_ch}), 1);
        end
        n_samples_g = labBroadcast (1, n_samples_l);
        n_data_per_event_hb_g = labBroadcast (1, n_data_per_event_hb_l);
        n_data_per_event_tb_g = labBroadcast (1, n_data_per_event_tb_l);
    else
        n_channels_g = labBroadcast (1);
        n_par_hb_g = labBroadcast (1);
        n_par_hb_g = labBroadcast (1);
        ae_chars_hb_g = labBroadcast (1);
        ae_chars_tb_g = labBroadcast (1);
        n_samples_g = labBroadcast (1);
        n_data_per_event_hb_g = labBroadcast (1);
        n_data_per_event_tb_g = labBroadcast (1);
    end
    labBarrier ();
    n_events_tb_g = gcat (length (local_timebased.tot));
    n_events_total_tb_g = gplus (length (local_timebased.tot));
    n_events_hb_l = zeros(n_channels_g, 1);
    for ch=1:n_channels_g
        n_events_hb_l(ch, 1) = ...
            size(local_hitbased(ch).tot, 2);
    end
    n_events_hb_g = gcat (n_events_hb_l, 2);
    n_events_total_hb_g = gplus (n_events_hb_l);
    labBarrier ();
end
n_events_tb_g = n_events_tb_g{1};
n_events_total_tb_g = n_events_total_tb_g{1};
n_par_tb_g = n_par_tb_g{1};
n_events_hb_g = n_events_hb_g{1};
n_events_total_hb_g = n_events_total_hb_g{1};
n_data_per_event_hb_g = n_data_per_event_hb_g{1};
n_data_per_event_tb_g = n_data_per_event_tb_g{1};
n_channels_g = n_channels_g{1};
ae_chars_tb_g = ae_chars_tb_g{1};
ae_chars_hb_g = ae_chars_hb_g{1};

d_timebased.tot = [];
d_timebased.parametrics = [];
spmd
    tmp_tot = codistributed.build (local_timebased.tot, ...
        codistributor1d (2, ...
        n_events_tb_g, ...
        [1 n_events_total_tb_g]));
    tmp_parametrics = ...
        codistributed.build (local_timebased.parametrics, ...
        codistributor1d (2, ...
        n_events_tb_g , ...
        [n_par_tb_g n_events_total_tb_g]));
end

d_timebased.tot = tmp_tot;
d_timebased.parametrics = tmp_parametrics;

for ch=1:n_channels_g
    for fn=1:length(ae_chars_hb_g)
        if (~strcmp (ae_chars_hb_g {fn}, 'x'))
            spmd
                 tmp =...
                    codistributed.build (local_hitbased (ch).(ae_chars_hb_g {fn}), ...
                    codistributor1d (2, ...
                    n_events_hb_g(ch,:), ...
                    [n_data_per_event_hb_g(ch, fn) ...
                    n_events_total_hb_g(ch)]));
            end
          d_hitbased(ch).(ae_chars_hb_g {fn}) = tmp;
        end
    end
    for fn=1:length (ae_chars_tb_g)
        spmd
            tmp = ...
                codistributed.build (local_timebased.ae_characteristics(ch).(ae_chars_tb_g{fn}), ...
                codistributor1d (2, ...
                n_events_tb_g, ...
                [n_data_per_event_tb_g(fn) ...
                n_events_total_tb_g]));
        end
      d_timebased.ae_characteristics(ch).(ae_chars_tb_g{fn}) = tmp;
    end
end
end


