require "ifj21"

function main ()
    local a : integer
    if a == nil then
        local b : integer = a + a
        write(b)
    else
        write("isnotnil")
    end
end

main()
