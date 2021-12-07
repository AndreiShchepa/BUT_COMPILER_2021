require("ifj21")

function foo()
   local x = 4
   while 1 do
      x = x * 2
      if x > 8 then
         write(x)
         return x
      else
      end
   end
end

function main()
   local a = foo()
   write(a)
end

main()
