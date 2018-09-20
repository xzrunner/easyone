-- create node RoundedRectangle

require "moon.sg"

local node = moon.sg.new_node("sg_rounded_rect")
if node then
	node:set_pos(moon.mouse.get_position())
end