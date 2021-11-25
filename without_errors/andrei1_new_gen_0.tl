require "ifj21"

function foo()
    local b : integer = 5
    while b > 0 do
        local a : integer = 1000
        write(b, "\n", a, "\n")

        if (b == 1) then
            while b > 0 do
                local b : integer = 2
                write(b, "\n")
            end

            write(b, "\n")
        else end
        b = b - 1
    end

    write(b, "\n")
end

foo()
