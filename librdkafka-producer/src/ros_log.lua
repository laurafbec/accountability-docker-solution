-- Chisel description
description = "ROS events logging"
short_description = "ROS events logging"
category = "misc"
local config = require "settings"

-- Chisel argument list
args = {}

json = require ("dkjson")

function on_init()
	evt_num = chisel.request_field("evt.num")
        datetime = chisel.request_field("evt.datetime")

	container_id = chisel.request_field("container.id")
	container_name = chisel.request_field("container.name")

	proc_name = chisel.request_field("proc.name")
	proc_pid = chisel.request_field("proc.pid")
	proc_env = chisel.request_field("proc.env")


	-- Client and server IP address and ports
	fd_cip = chisel.request_field("fd.cip")
	fd_cport = chisel.request_field("fd.cport")
	fd_sip = chisel.request_field("fd.sip")
	fd_sport = chisel.request_field("fd.sport")

	user_name = chisel.request_field("user.name")

        evt_type = chisel.request_field("evt.type")
        evt_arg_data = chisel.request_field("evt.arg.data")

        sysdig.set_snaplen(56000)
        chisel.set_filter(config.filter)


	return true
end

-- Event parsing callback
function on_event()
	local evt_num = evt.field(evt_num)
	local datetime = evt.field(datetime)

	local container_id = evt.field(container_id)
	local container_name = evt.field(container_name)

	local proc_name = evt.field(proc_name)
	local proc_pid = evt.field(proc_pid)
	local proc_env = evt.field(proc_env)

	local ros_master_uri = string.match(proc_env, "ROS_MASTER_URI=(%S+)")
	local ros_hostname = string.match(proc_env, "ROS_HOSTNAME=(%S+)")

	local fd_cip = evt.field(fd_cip)
	local fd_cport = evt.field(fd_cport)
	local fd_sip = evt.field(fd_sip)
	local fd_sport = evt.field(fd_sport)

	local user_name = evt.field(user_name)

	local evt_type = evt.field(evt_type)
	local evt_arg_data = evt.field(evt_arg_data)
	
	-- variable to store node name
	local node_name
	
	-- variable to store list of topics in "[topics: /<topic1>, /<topic2>+]"
	local topics
	-- variable to store topic in "topic[/<topic>]"
	local topic
	--  variable to store topic type
	local topic_type
	-- variable to store the final list of topics
	local topics_list = ""
        -- variables to store line and function name in python write message "line <line_numer>, in <function_name>."
	local line
	local function_name

	local msg_packet
	local msg_type

	local test = "[topics: /rosout, /chatter, /prueba-topic]"

	if not (evt_arg_data == nil) then
		-- pattern: init_node, name[/listener_30253_1614879975323]
		node_name = string.match(evt_arg_data, ("init_node, name%[/(%S+)]"))
		-- pattern: callerid=/listener_30253_1614879975323....latching
		if (node_name == nil) then
			node_name = string.match(evt_arg_data, ("callerid=/(%S+)%.%.%.%.latching"))
		end
		-- pattern: +SERVICE [/talker_30231_1614879973919/set_logger_level]
		if (node_name == nil) then
			node_name = string.match(evt_arg_data, ("SERVICE %[/(%S+)/"))
		end
				
		topics = string.match(evt_arg_data, ("%[topics:.*]"))
				
		if not (topics == nil) then
			for w in string.gmatch (topics, "%s/(%S+),?") do
				if not (topics_list == "") then
					topics_list = topics_list.." "..w
				else
					topics_list = w
				end
			end
		end
		-- delete end bracket
		topics_list = topics_list:sub(1, -2)
		
		-- pattern topic[/rosout] adding connection
		topic = string.match(evt_arg_data, ("topic%[/(%S+)]"))
		if not (topic == nil) then
			topics_list = topic
		end

		-- pattern: topic [/chatter] type [std_msgs/String]
		topic, msg_packet, msg_type = string.match(evt_arg_data, ("topic%s%[/(%S+)]%stype%s%[(%S+)/(%S+)]"))
		if not (topic == nil) then
			topics_list=topic
			msg_packet = msg_packet .. "/" .. msg_type
		end

		line,function_name = string.match(evt_arg_data, "line (%d+), in (%S+).")

	end

	local log_level_dict = {'DEBUG','INFO','WARN','ERROR','FATAL'}
	if not (evt_arg_data == nil) then
		for _,v in pairs( log_level_dict ) do
			if evt_arg_data:find( v ) then
				log_level = v
				break
			end
		end
	end
	
	if (topics_list == "") then
		topics_list = nil
	end

        if not (evt_arg_data == nil or log_level == nil) then
          print(json.encode({evt_num=evt_num, datetime=datetime, container_id=container_id, container_name=container_name, proc_name=proc_name, proc_pid=proc_pid, evt_type=evt_type, evt_arg_data=evt_arg_data, user_name=user_name, fd_cip=fd_cip, fd_cport=fd_cport, fd_sip=fd_sip, fd_sport=fd_sport, ros_master_uri=ros_master_uri, ros_hostname=ros_hostname, log_level=log_level, line=line, function_name=function_name, topics=topics_list, node_name=node_name, msg_packet=msg_packet}))
	end
        return true
end

-- End of capture callback
function on_capture_end()
	return true
end




