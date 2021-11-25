require("ifj21")
function main()
   local s1 = "Toto je nejaky text"
   local s2 = s1 .. ", ktery jeste trochu obohatime"
   write(s1, "\010", s2, "\n")

   local s1len = #s1
   local s1len4 = #s1 - 3
   s1 = substr(s2, s1len4, s1len)
   write("4 znaky od ", s1len4, ". znaku v \"", s2, "\":", s1, "\n")

   s1 = substr(s2, 16, 19)
   s1len4 = #s1
   write(s1len4, " znaky od ", 16, ". znaku v \"", s2, "\":", s1, "\n")
end
main()
