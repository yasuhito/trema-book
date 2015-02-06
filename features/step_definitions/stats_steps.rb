Then(/^the total number of tx packets should be:$/) do |table|
  sleep 1
  table.hashes[0].each_pair do |host, n|
    expect(count_packets(`trema show_stats #{host} --tx`)).to eq(n.to_i)
  end
end

Then(/^the total number of rx packets should be:$/) do |table|
  sleep 1
  table.hashes[0].each_pair do |host, n|
    expect(count_packets(`trema show_stats #{host} --rx`)).to eq(n.to_i)
  end
end
