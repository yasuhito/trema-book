require 'active_support/core_ext/class/attribute_accessors'
require 'slice'
require 'trema'

# List of shortest-path flow entries.
class Path < Trema::Controller
  cattr_accessor(:all, instance_reader: false) { [] }

  def self.create(shortest_path, packet_in)
    new.save(shortest_path, packet_in).tap { |new_path| all << new_path }
  end

  def self.destroy(path)
    all.delete path
  end

  def self.find(&block)
    all.select { |each| block.call(each) }
  end

  attr_reader :slice

  def slice=(name)
    Slice.find_by!(name: name)
    @slice = name
  end

  attr_reader :packet_in

  def save(full_path, packet_in)
    @full_path = full_path
    @packet_in = packet_in
    logger.info 'Creating path: ' + @full_path.map(&:to_s).join(' -> ')
    flow_mod_add_to_each_switch
    self
  end

  def destroy
    logger.info 'Deleting path: ' + @full_path.map(&:to_s).join(' -> ')
    Path.destroy self
    flow_mod_delete_to_each_switch
  end

  def port?(port)
    path.include? port
  end

  def endpoints
    [@full_path[0..1], @full_path[-2..-1].reverse]
  end

  def link?(*link)
    flows.any? { |each| each.sort == link.sort }
  end

  def out_port
    path.last
  end

  private

  def flows
    path[1..-2].each_slice(2).to_a
  end

  def flow_mod_add_to_each_switch
    path.each_slice(2) do |in_port, out_port|
      send_flow_mod_add(out_port.dpid,
                        match: exact_match(in_port.number),
                        actions: SendOutPort.new(out_port.number))
    end
  end

  def flow_mod_delete_to_each_switch
    path.each_slice(2) do |in_port, out_port|
      send_flow_mod_delete(out_port.dpid,
                           match: exact_match(in_port.number),
                           out_port: out_port.number)
    end
  end

  def exact_match(in_port)
    ExactMatch.new(@packet_in).tap { |match| match.in_port = in_port }
  end

  def path
    @full_path[1..-2]
  end
end
