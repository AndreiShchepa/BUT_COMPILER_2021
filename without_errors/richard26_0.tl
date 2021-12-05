require "ifj21"
function az_tu(       word11 : string,  word22 : string,  word33 : string,
              word44 : string,  word55 : string,  word66 : string) : string
        local sentence1 : string = word11 .. word22 .. word33 .. word44 .. word55 .. word66
        return sentence1
    end
function sconcatenuj( word1 : string,  word2 : string,  word3 : string,
              word4 : string,  word5 : string,  word6 : string) : string
        local sentence0 : string = az_tu(word1, word2, word3, word4, word5, word6)
        return sentence0
    end
function main ()
    local sentence : string = "AndreiJeNajvacsiFrajerNaSkole"
    write(sentence, "\n")
    local word1 : string = substr(sentence, 1, 6)
    write(word1, "\n")

    local word2 : string = substr(sentence, 7, 8)
    write(word2, "\n")

    local word3 : string = substr(sentence, 9, 16)
    write(word3, "\n")

    local word4 : string = substr(sentence, 17, 22)
    write(word4, "\n")

    local word5 : string = substr(sentence, 23, 24)
    write(word5, "\n")

    local word6 : string = substr(sentence, 25, 29)
    write(word6, "\n")

    local concatenate : string = sconcatenuj(word1, word2, word3, word4, word5, word6)
    write(concatenate, "\n")
end

main()

