-- create node Twirl

require "moon.sg"

local node = moon.sg.new_node("sg_twirl")
if node then
	node:set_pos(moon.mouse.get_position())
end