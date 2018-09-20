-- create node Checkerboard

require "moon.sg"

local node = moon.sg.new_node("sg_checkerboard")
if node then
	node:set_pos(moon.mouse.get_position())
end