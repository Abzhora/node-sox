{
  "targets": [
    {
      "target_name": "sox",
      "sources": [ "sox.cc" ],
      "libraries" : [ "-lsox" ]
    },
    {
      "target_name": "format",
      "sources": [ "src/format.cc" ],
      "libraries" : [ "-lsox" ]
    }
  ]
}
