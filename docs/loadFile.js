var open_file = function(e) {
  var successArray = [];

  for(var i = 0; i < e.target.files.length; i++) {
    const file_reader = new FileReader();
    file_reader.fileName = e.target.files[i].name;
    file_reader.onload = load_file(event, file_reader.fileName);
    successArray.push(e.target.files[i].name);
  }

  // Some messages to indicate to the user some success.
  console.log("Successfully loaded " + successArray.join(" ") + "\n");
  alert("Successfully loaded " + successArray.join(" ") + "\n");
};

var load_file = function(event, fileName) {
  const uint8Arr = new Uint8Array(event.target.result);
  // This allows to upload savefiles to the IDBFS database. Fails if "/local/Save" does not exist!
  FS.writeFile('/local/Save/' + fileName, uint8Arr);

  // Synchronise the IDBFS to update it.
  FS.syncfs(function (err) {
    assert(!err);
  });
};
