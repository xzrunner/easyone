-- create node ChannelMask

require "moon.sg"

local node = moon.sg.new_node("sg_channel_mask")
if node then
	node:set_pos(moon.mouse.get_position())
end