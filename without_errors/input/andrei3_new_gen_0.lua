require("ifj21")

function reads_3()
   local id1 = reads()
   local id2 = readn()
   local id3 = readi()

   return id1, id3, id2
end

function main(id1, id2, id3)
   write(id1, " : \n", id2, "\n", id3, "\n")

   id1, id2, id3 = reads_3()

   return
end

main("Default numbers", 10, 1.0e-2)
