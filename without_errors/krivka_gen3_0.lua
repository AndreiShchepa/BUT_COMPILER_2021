
function f(x)
   if x < 10 then return x - 1
   else
      x = x - 1
      write("calling g with ", x, "\n")
      local res = g(x)
      return res
   end
end

function g(x)
   if x > 0 then
      write("calling f with ", x, "\n")
      local x = f(x)
      return x
   else return 200 end
end

function main()
   local res = g(10)
   write("res: ", res, "\n")
end; main()
