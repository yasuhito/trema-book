require 'aruba/cucumber'

# show_stats output format:
# ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
def count_packets(stats)
  return 0 if stats.split.size <= 1
  stats.split[1..-1].inject(0) do |sum, each|
    sum + each.split(',')[4].to_i
  end
end
