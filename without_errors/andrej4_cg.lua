require("ifj21")

function main()
   local int1 = 1
   local int2 = 1
   local flt1 = 1.0
   local flt2 = 1.0
   local num

   num = int1 - flt1
   num = flt1 - int1
   num = int1 - int2
   num = flt1 - flt2

   num = int1 + flt1
   num = flt1 + int1
   num = int1 + int2
   num = flt1 + flt2

   num = int1 * flt1
   num = flt1 * int1
   num = int1 * int2
   num = flt1 * flt2

   num = int1 / flt1
   num = flt1 / int1
   num = int1 / int2
   num = flt1 / flt2


   local ll = int1 // int2



end

main()
