require "ifj21"

function foo() : string
    local a : string = "str"
    return a
end

function main ()
    --if nil == nil then else
    --end
    local b : string = foo()
    local a : string = "aaaaaaa"
    write(a)
    write(b)
    write(009E4)
end

main()
