require "ifj21"

function reads_3() : string, integer, number
    local id1 : string  = reads()
    local id2 : number  = readn()
    local id3 : integer = readi()

    return id1, id3, id2
end

function main(id1 : string, id2 : integer, id3 : number)
    write(id1, " : \n", id2, "\n", id3, "\n")

    id1, id2, id3 = reads_3()

    return
end

main("Default numbers", 10, 1.0e-2)

global reads_3 : function() : string, integer, number
