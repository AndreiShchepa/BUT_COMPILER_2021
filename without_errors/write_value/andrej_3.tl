require "ifj21"

function foo(x: integer, y: integer, z: integer, t: integer) : integer, integer, integer
local a : integer = x
local b : integer = y
local c : integer = z + t
return a, b
end

function main()
local q1 : integer
local q2 : integer
local q3 : integer

local p1 : integer = 1
local p2 : integer = 2
local p3 : integer = 3
local p4 : integer = 4

q1, q2, q3 = foo(p1, p2, p3, p4)
write(q1 , "\n", q2 , "\n", q3 , "\n")

q1, q2, q3 = foo(p1, p2, p3, p4)
write(q1 , "\n", q2 , "\n", q3 , "\n")

end

main()



