-- create node Or

require "moon.sg"

local node = moon.sg.new_node("sg_or")
if node then
	node:set_pos(moon.mouse.get_position())
end