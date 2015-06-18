-- tolua: typedef class
-- Written by Waldemar Celes
-- TeCGraf/PUC-Rio
-- Jul 1998
-- $Id: typedef.lua,v 1.2 2001/11/26 23:00:27 darkgod Exp $

-- This code is free software; you can redistribute it and/or modify it.
-- The software provided hereunder is on an "as is" basis, and
-- the author has no obligation to provide maintenance, support, updates,
-- enhancements, or modifications. 



-- Typedef class
-- Represents a type synonym.
-- The 'de facto' type replaces the typedef before the
-- remaining code is parsed.
-- The following fields are stored:
--   utype = typedef name
--   type = 'de facto' type
--   mod = modifiers to the 'de facto' type
classTypedef = {
 utype = '',
 mod = '',
 type = ''
}

-- Print method
function classTypedef:print (ident,close)
 print(ident.."Typedef{")
 print(ident.." utype = '"..self.utype.."',")
 print(ident.." mod = '"..self.mod.."',")
 print(ident.." type = '"..self.type.."',")
 print(ident.."}"..close)
end

-- Internal constructor
function _Typedef (t)
 t._base = classTypedef
 settag(t,tolua_tag)
 appendtypedef(t)
 return t
end

-- Constructor
-- Expects one string representing the type definition.
function Typedef (s)
 if strfind(s,'[%*&]') then
  tolua_error("#invalid typedef: pointers (and references) are not supported")
 end
 local t = split(gsub(s,"%s%s*"," ")," ")
 return _Typedef {
  utype = t[t.n],
  type = t[t.n-1],
  mod = concat(t,1,t.n-2)
 }
end


