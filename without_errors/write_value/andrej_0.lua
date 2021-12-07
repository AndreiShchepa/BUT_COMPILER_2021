require("ifj21")


function foo2(x, y, z)
   local a = 0
   while x > 0 do
      a = a + y + z
      x = x - 1
   end
   return a
end

function foo(a, b)
   local x = foo2(b, a, b)
   return x
end

function main()
   local a = 10
   local c
   while a > 0 do
      local b = 10
      while b > 0 do
         if a > 5 then
            if b > 5 then
               c = foo(a, b)
            else
               write(c)
            end
         else
            write(a)
         end
         b = b - 1
      end
      a = a - 1
   end
end

main()
