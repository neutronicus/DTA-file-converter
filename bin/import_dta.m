function [a b m] = import_dta(filename, options)

if nargin < 3
  options = logical([1 1 1 1])
end

if nargout == 0
  import_dta_private(filename, {'a' 'b' 'm'}, ...
                     options, 'base');
else
  import_dta_private (filename, {'tmp1' 'tmp2' 'tmp3'}, options, 'caller');
  a = tmp1; b = tmp2; m = tmp3;
end

end