function a = get_ae_char(dist_array, char_name, channel_num)

nlabs = matlabpool('size');
ae_char_dims = zeros(1,nlabs);

char_names = {'rise_time',
              'counts_to_peak',
              'total_counts',
              'energy_counts',
              'duration',			
              'amplitude',
              'rms_voltage',
              'asl',
              'gain',
              'threshold',			
              'pre_amp_current',
              'lost_hits',
              'avg_freq',
              'tot',				
              'rms_voltage',
              'reverb_freq',
              'init_freq',
              'signal_strength',	
              'abs_energy',
              'partial_power',
              'freq_centroid',
              'freq_peak'}

valid_name = 0;
for ind=1:numel (char_names)
  valid_name = valid_name | strcmp (char_name, (char_names {ind}));
end
if ~valid_name
  error ('name does not correspond to a recognized AE characteristic');
end

spmd
L = getLocalPart(dist_array);

ae_char = [L(:,channel_num).(char_name)];
d_l = length(ae_char);
end

for n=1:nlabs
  ae_char_dims (n) = d_l {n};
end

spmd
a = codistributed.build(ae_char, codistributor1d(2, ae_char_dims, [1 sum(ae_char_dims)]));
end