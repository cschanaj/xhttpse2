"use strict";

const child_process = require("child_process");
const fs = require("fs");
const os = require("os");
const path = require("path");
const process = require("process");

function parseXmlByRegex(content, regex, postProcessingCallback) {
  let matches = null;
  while ((matches = regex.exec(content)) !== null) {
    // This is necessary to avoid infinite loops with zero-width matches
    if (matches.index === regex.lastIndex) {
      regex.lastIndex++;
    }
    postProcessingCallback(matches);
  }
}

// check the number of arguments
if (process.argv.length <= 2) {
  console.log("Usage: node ruleset-update.js [FILE]");
  process.exit();
}

// create a backup of the input FILE if it does not exist
const filename = process.argv[2];
const backupFilename = filename + ".bak";
if (!fs.existsSync(backupFilename)) {
  fs.copyFileSync(filename, backupFilename);
}

// read the content of the input FILE
const content = fs.readFileSync(filename, { encoding: "utf8" });

let rulesetName = "";
let domains = [];

// parse content to find the ruleset name in the FILE
parseXmlByRegex(content, /^\s*<ruleset\s*name="([^"]*)"[^>]*>$/gm, matches => {
  rulesetName = matches[1];
});

// parse content to find domains in the FILE
parseXmlByRegex(content, /^\s*-\s*([^>\n]*)$/gm, matches => {
  domains.push(matches[1]);
});

parseXmlByRegex(
  content,
  /^\s*<target\s*host="([^"]*)"\s*\/>$/gm,
  matches => {
    domains.push(matches[1]);
  }
);

// sort domains alphabetically, from top level domains to lower ones
domains = domains
  .map(domain =>
    domain
      .split(".")
      .reverse()
      .join(".")
  )
  .sort()
  .map(domain =>
    domain
      .split(".")
      .reverse()
      .join(".")
  );

// create temporary file
const tmpFilename = path.join(
  os.tmpdir(),
  "FUXMERANDM-" + Date.now().toString()
);

fs.writeFileSync(tmpFilename, domains.join("\n"), { encoding: "utf8" });

// rewrite the ruleset FILE completely
const command = `${__dirname}/../bin/xhttpse --num-threads 16 --name "${rulesetName}" -o "${filename}" "${tmpFilename}"`;
child_process.execSync(command);

// remove temporary file
fs.unlinkSync(tmpFilename);
