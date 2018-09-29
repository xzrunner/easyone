-- create node Rejection

require "moon.sg"

local node = moon.sg.new_node("sg_rejection")
if node then
	node:set_pos(moon.mouse.get_position())
end