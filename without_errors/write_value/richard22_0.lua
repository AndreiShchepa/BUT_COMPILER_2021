require("ifj21")

function najvacsi_numb(a, b, c)
   if a >= b then
      if a >= c then
         return a
      else
         return c
      end
      if b >= c then
         return b
      else
         return c
      end
      if b >= c then
         return b
      else
         return c
      end
   else
      if b >= c then
         return b
      else
         return c
      end


   end
end

function main()
   local a = 8
   local b = 6
   local c = 10
   local najvacsi = 0
   najvacsi = najvacsi_numb(a, b, c)
   write(najvacsi)
end

main()
