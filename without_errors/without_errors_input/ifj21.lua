local _tl_compat; if (tonumber((_VERSION or ''):match('[%d.]*$')) or 0) < 5.3 then local p, m = pcall(require, 'compat53.module'); if p then _tl_compat = m end end; local io = _tl_compat and _tl_compat.io or io; local math = _tl_compat and _tl_compat.math or math; local pcall = _tl_compat and _tl_compat.pcall or pcall; local string = _tl_compat and _tl_compat.string or string

function tointeger(f)
   local _
   local ret
   if type(f) == "number" or type(f) == "integer" then
      ret, _ = math.modf(f)
   end
   return ret
end

function readi()
   local ret
   local _
   local rest
   ret, _ = io.read("*n")
   rest, _ = io.read("*l")
   if string.match(rest, "%g") ~= nil then
      return nil
   end
   return math.tointeger(ret)
end

function readn()
   local ret
   local _
   local rest
   ret, _ = io.read("*n")
   rest, _ = io.read("*l")
   if string.match(rest, "%g") ~= nil then
      return nil
   end
   return tonumber(ret)
end

function reads()
   local ret
   local _
   ret, _ = io.read("*l")
   return ret
end

function write(...)
   local va = { n = select("#", ...), ... }
   for i = 1, va.n do
      local x = va[i]

      if type(x) == "number" then
         if (pcall(function() string.format("%i", x) end)) then
            if string.find(tostring(x), ".", 1, true) == nil then
               io.write(string.format("%i", x))
            else
               io.write(string.format("%a", x))
            end
         else
            io.write(string.format("%a", x))
         end
      elseif type(x) == "integer" then
         io.write(string.format("%i", x))
      else
         io.write(tostring(x))
      end
   end
end

function substr(str, i, j)
   if str == nil or i == nil or j == nil then
      error("ERROR 8: Unexpected nil value in the parameters.")
   end
   if i <= 0 or i > #str and j <= 0 or j > #str then
      return ""
   end
   return string.sub(str, i, j)
end

function ord(s, i)
   if s == nil or i == nil then
      error("ERROR 8: Unexpected nil value in the parameters.")
   end
   if i <= 0 or i > #s then
      return nil
   end
   return string.byte(s, i)
end

function chr(i)
   if i == nil then
      error("ERROR 8: Unexpected nil value in the parameter.")
   end
   if i < 0 or i > 255 then
      return nil
   end
   return string.char(i)
end
