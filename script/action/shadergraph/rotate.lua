-- create node Rotate

require "moon.sg"

local node = moon.sg.new_node("sg_rotate")
if node then
	node:set_pos(moon.mouse.get_position())
end