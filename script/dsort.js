"use strict";

const fs = require("fs");
const process = require("process");

const tldsort = require("./libs/tldsort");

// check the number of arguments
if (process.argv.length <= 2) {
  console.log("Usage: node dsort.js [FILE]");
  process.exit();
}

// read the content of the input FILE
const filename = process.argv[2];
const content = fs.readFileSync(filename, { encoding: "utf8" });
const domains = content
  .split("\n")
  .map((domain) => domain.trim())
  .filter((domain) => domain.length > 0);

tldsort(domains).forEach((domain) => {
  console.log(domain);
});
