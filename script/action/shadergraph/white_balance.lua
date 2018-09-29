-- create node WhiteBalance

require "moon.sg"

local node = moon.sg.new_node("sg_white_balance")
if node then
	node:set_pos(moon.mouse.get_position())
end