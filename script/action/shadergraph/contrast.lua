-- create node Contrast

require "moon.sg"

local node = moon.sg.new_node("sg_contrast")
if node then
	node:set_pos(moon.mouse.get_position())
end