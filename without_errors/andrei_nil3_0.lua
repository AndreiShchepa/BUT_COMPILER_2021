require("ifj21")

function main()
   local id1 = "str"
   local id2 = "Str2"
   local id3 = "Str3"
   local id4 = "Str4"
   local id5 = "Str5"
   id1 = id2 .. id3 .. id4 .. id5 .. nil .. id1
end

main()
