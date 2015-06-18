-- tolua: code class
-- Written by Waldemar Celes
-- TeCGraf/PUC-Rio
-- Jul 1999
-- $Id: code.lua,v 1.2 2001/11/26 23:00:23 darkgod Exp $

-- This code is free software; you can redistribute it and/or modify it.
-- The software provided hereunder is on an "as is" basis, and
-- the author has no obligation to provide maintenance, support, updates,
-- enhancements, or modifications. 


-- Code class
-- Represents Lua code to be compiled and included
-- in the initialization function.
-- The following fields are stored:
--   text = text code
classCode = {
 text = '',
 _base = classFeature,
}
settag(classCode,tolua_tag)

-- register code
function classCode:register ()
 -- clean Lua code
 local s = clean(self.text)
 if not s then
  error("parser error in embedded code")  
 end
 
 -- convert to C
 output('\n { /* begin embedded lua code */\n')
 output('  static unsigned char B[] = {\n   ')
 local t={n=0}
 local b = gsub(s,'(.)',function (c) 
                         local e = '' 
                         %t.n=%t.n+1 if %t.n==15 then %t.n=0 e='\n   ' end 
                         return format('%3u,%s',strbyte(c),e) 
                        end
               )
 output(b..strbyte(" "))
 output('\n  };\n')
 output('  lua_dobuffer(tolua_S,(char*)B,sizeof(B),"tolua: embedded Lua code");')
 output(' } /* end of embedded lua code */\n\n')
end
 

-- Print method
function classCode:print (ident,close)
 print(ident.."Code{")
 print(ident.." text = [["..self.text.."]],")
 print(ident.."}"..close)
end


-- Internal constructor
function _Code (t)
 t._base = classCode
 settag(t,tolua_tag)
 append(t)
 return t
end

-- Constructor
-- Expects a string representing the code text
function Code (l)
 return _Code {
  text = l
 }
end


