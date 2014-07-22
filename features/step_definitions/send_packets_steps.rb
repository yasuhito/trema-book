# encoding: utf-8

When(/^I send (\d+) packets from (.+) to (.+)$/) do |n_pkts, host_a, host_b|
  step "I run `trema send_packets -s #{host_a} -d #{host_b} --n_pkts #{n_pkts}`"
end

When(/^I send 1 packet from (.+) to (.+)$/) do |host_a, host_b|
  step "I send 1 packets from #{host_a} to #{host_b}"
end
