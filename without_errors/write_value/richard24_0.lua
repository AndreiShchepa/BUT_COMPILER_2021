require("ifj21")
function main()
   local sentence = "AndreiJeNajvacsiFrajerNaSkole"
   local word5 = substr(sentence, 0, 5)
   local word6 = substr(sentence, 6, 7)
   local word1 = substr(sentence, 8, 15)
   local word2 = substr(sentence, 16, 21)
   local word3 = substr(sentence, 22, 23)
   local word4 = substr(sentence, 24, 28)
   write(word1, word2, word3, word4, word5, word6)
end

main()
