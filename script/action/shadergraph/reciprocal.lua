-- create node Reciprocal

require "moon.sg"

local node = moon.sg.new_node("sg_reciprocal")
if node then
	node:set_pos(moon.mouse.get_position())
end