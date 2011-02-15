function [a b x m] = import_dta(filename)

if nargout == 0
  import_dta_private(filename, {'a' 'b' 'x' 'm'}, logical([1 1 1 1]));
else
  import_dta_private (filename, {'tmp1' 'tmp2' 'tmp3' 'tmp4'}, logical ...
                      ([1 1 1 1]));
  a = tmp1; b = tmp2; x = tmp3; m = tmp4;
end

end