require("ifj21")

function main()
   local a = 2
   local b = 2
   local c = 2
   while a > 0 do
      while c > 0 do
         write(b)
         c = c - 1
      end
      local b = 1
      write(b)
      a = a - 1
   end
end

main()
