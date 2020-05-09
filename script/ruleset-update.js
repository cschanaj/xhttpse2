"use strict";

const child_process = require("child_process");
const fs = require("fs");
const os = require("os");
const path = require("path");
const process = require("process");

const tldsort = require("./libs/tldsort");
const parseRuleset = require("./libs/parseRuleset");
const XHTTPSE_PATH = process.env.XHTTPSE_PATH || "../bin/xhttpse";

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
const xmlContent = fs.readFileSync(filename, { encoding: "utf8" });
const ruleset = parseRuleset(xmlContent);

console.log(JSON.stringify(ruleset, null, 2));
process.exit();

// sort domains by top-level domains alphabetically, remove duplicates
ruleset.target.hosts = tldsort(ruleset.target.hosts);

// create temporary file
const tmpFilename = path.join(
  os.tmpdir(),
  "FUXMERANDM-" + Date.now().toString()
);

fs.writeFileSync(tmpFilename, ruleset.target.hosts.join("\n"), {
  encoding: "utf8",
});

// construct commands from raw parts to rewrite the ruleset completely
const rawCommandParts = [
  XHTTPSE_PATH,
  "--num-threads " + os.cpus().length,
  ruleset.securecookie ? "--securecookie" : null,
  "--name " + `"${ruleset.metadata.name}"`,
  "-o " + `"${filename}"`,
  `"${tmpFilename}"`,
];

const command = rawCommandParts.filter((part) => part !== null).join(" ");
child_process.execSync(command);

// remove temporary file
fs.unlinkSync(tmpFilename);
