require("ifj21")

function main1(a, b, str, c, d)
   local id = a + c + 12 + 34
   local id1 = b + d - 90 + 78 * 54
   local id2 = str .. "string11111"
   write(id, id1, id2)
end

main1(12, 13.5, "andr", 324, 90.899)
