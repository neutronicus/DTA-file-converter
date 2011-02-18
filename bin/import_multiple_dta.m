function [d_hitbased d_timebased] = import_multiple_dta ( files )
%IMPORT_MULTIPLE_DTA codistributed array from .dta filename array
%   This function will return two codistributed arrays - one with
%   all of the hit-based data and one with all of the time-based
%   data.  This occurs in parallel.

    n = length(files);
    nlabs = matlabpool('size');
    local_hitbased = [];
    local_timebased = [];
    n_channels = 16;

    
    disp (mod (n, nlabs));
    if (mod (n, nlabs) ~= 0)
      n_files_to_process = ...
          ([1:nlabs] <= mod (n, nlabs)) .* ceil (n/nlabs) ...
          + ([1:nlabs] > mod (n, nlabs)) .* floor (n/nlabs);
    else
      n_files_to_process = ceil (n/nlabs);
    end
    offsets = circshift (n_files_to_process, [0 1]);
    offsets (1) = 0;
    
    hb_dims = zeros(1,nlabs);
    tb_dims = zeros(1,nlabs);
    disp (n_files_to_process);
    disp (offsets);
    
    spmd
        for ind=1:n_files_to_process (labindex)
          [a, b, x, m] = import_dta( files {offsets (labindex) + ind} );
          disp (a);
          local_hitbased = [local_hitbased; a];
          local_timebased = [local_timebased b];
        end
        n_l_h = size(local_hitbased,1);
        n_l_t = size(local_timebased,2);
        disp(local_hitbased);
        disp(local_timebased);
    end
    
    for ind=1:nlabs
        hb_dims(ind) = n_l_h{ind};
        tb_dims(ind) = n_l_t{ind};
    end
    
    spmd
        disp (size (local_hitbased));
        disp (hb_dims);
        disp (sum (hb_dims));
        d_hitbased = codistributed.build(local_hitbased, ...
            codistributor1d(1, hb_dims, [sum(hb_dims) size(local_hitbased, 2)]));
        d_timebased = codistributed.build(local_timebased, ...
            codistributor1d(2, tb_dims, [1 sum(tb_dims)]));
    end
end