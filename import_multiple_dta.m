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
    
    hb_dims = zeros(1,nlabs);
    tb_dims = zeros(1,nlabs);
    
    spmd
        for i=0:floor(n/numlabs)
            if i + labindex <= n
                [a, b] = import_dta(char(files(i + labindex)));
                local_hitbased = [local_hitbased a];
                local_timebased = [local_timebased b];
            end
        end
        n_l_h = size(local_hitbased,2);
        n_l_t = size(local_timebased,2);
        disp(local_hitbased);
        disp(local_timebased);
    end
    
    for i=1:nlabs
        hb_dims(i) = n_l_h{i};
        tb_dims(i) = n_l_t{i};
    end
    
    spmd
        disp(hb_dims);
        disp(tb_dims);
        d_hitbased = codistributed.build(local_hitbased, ...
            codistributor1d(2, hb_dims, [n_channels sum(hb_dims)]));
        d_timebased = codistributed.build(local_timebased, ...
            codistributor1d(2, tb_dims, [1 sum(tb_dims)]));
    end
end