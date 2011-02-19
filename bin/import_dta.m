function [a b x m] = import_dta(filename)

if nargout == 0
  import_dta_private(filename, {'a' 'b' 'm'}, ...
                     logical([1 1 1 1]), 'base');
else
  import_dta_private (filename, {'tmp1' 'tmp2' 'tmp3'}, logical([1 1 1 1]), 'caller');
  a = tmp1; b = tmp2; m = tmp3;
end

end