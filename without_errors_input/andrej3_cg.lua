require("ifj21")

function main()
   write("Zadaj string\n")
   local a = reads()

   write("Zadaj int\n")
   local b = readi()

   write("Zadaj float\n")
   local c = readn()

   write(a, "\n", b, "\n", c, "\n")

   local d = tointeger(c)

   write("int: ", d, "\n")

   local numInt = d // 5

   local stringMY = "Hello World"

   stringMY = substr(stringMY, 7, 12)

   local myNil = substr(stringMY, 7, 12)

   if myNil ~= nil then
      write("ERROR OPRAV SI KOD PEPEGA")
   else
      write("Zatial vsetko v poradecku")
   end
end

main()
