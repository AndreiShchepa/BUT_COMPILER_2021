require("ifj21")
function az_tu(word11, word22, word33,
   word44, word55, word66)
   local sentence1 = word11 .. word22 .. word33 .. word44 .. word55 .. word66
   return sentence1
end
function sconcatenuj(word1, word2, word3,
   word4, word5, word6)
   local sentence0 = az_tu(word1, word2, word3, word4, word5, word6)
   return sentence0
end
function main()
   local sentence = "AndreiJeNajvacsiFrajerNaSkole"
   write(sentence, "\n")
   local word1 = substr(sentence, 1, 6)
   write(word1, "\n")

   local word2 = substr(sentence, 7, 8)
   write(word2, "\n")

   local word3 = substr(sentence, 9, 16)
   write(word3, "\n")

   local word4 = substr(sentence, 17, 22)
   write(word4, "\n")

   local word5 = substr(sentence, 23, 24)
   write(word5, "\n")

   local word6 = substr(sentence, 25, 29)
   write(word6, "\n")

   local concatenate = sconcatenuj(word1, word2, word3, word4, word5, word6)
   write(concatenate, "\n")
end

main()
