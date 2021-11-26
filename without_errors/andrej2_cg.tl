require "ifj21"

function main()
    write("Zadaj string\n")
    local a : string = reads()

    write("Zadaj int\n")
    local b : integer = readi()

    write("Zadaj float\n")
    local c : number = readn()

    write(a, "\n", b, "\n", c, "\n")

    local d : integer = tointeger(c)

    write("int: ", d, "\n")

    local numInt : number = d//5

    local stringMY : string = "Hello World"

    stringMY = substr(stringMY, 7, 12)

    local myNil : string =  substr(stringMY, 7, 12)

    if myNil ~= nil then
    write("ERROR OPRAV SI KOD PEPEGA")
    else
    write("Zatial vsetko v poradecku")
    end
end

main()
