require("ifj21")


function funcAndrei(idx)
   local str1 = "STR1"
   local str2 = "concat"
   local str3 = "STR2"

   str1 = str2 .. str2 .. str3
   write(str1, "\n")
   idx = ord(str1, idx)

   if idx == 83 then
      write("ord works good!\n")
   else
      write("ord doesnt work good!\n")
   end

   return idx

end

function main(idx)
   local i
   i = funcAndrei(idx)

   write(i, "\n")
end

main(1)
