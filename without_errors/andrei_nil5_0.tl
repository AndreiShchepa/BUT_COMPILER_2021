require"ifj21"

-- wrong arithm types
function main()
    local str : string = "home"
    local int : integer = 100
    local dec : number = 190.432
    local nil_var : nil

    local id : number = dec / nil_var
end

main()
