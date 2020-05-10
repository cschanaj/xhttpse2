"use strict";

const child_process = require("child_process");
const fs = require("fs");
const os = require("os");
const path = require("path");
const process = require("process");

const tldsort = require("./libs/tldsort");
const parseRuleset = require("./libs/parseRuleset");
const splitDomainsByTlds = require("./libs/splitDomainsByTlds");
const XHTTPSE_PATH = process.env.XHTTPSE_PATH || "../bin/xhttpse";

// check the number of arguments
if (process.argv.length <= 2) {
  console.log("Usage: node ruleset-update.js [FILE]");
  process.exit();
}

// create a backup of the input FILE if it does not exist
// path.normalize mess up files with underscore in their names
const filename = process.argv[2];
const backupFilename = filename + ".bak";
if (!fs.existsSync(backupFilename)) {
  fs.copyFileSync(filename, backupFilename);
}

// read the content of the input FILE
const xmlContent = fs.readFileSync(filename, { encoding: "utf8" });
const ruleset = parseRuleset(xmlContent);

// sort domains by top-level domains alphabetically, remove duplicates
ruleset.target.hosts = tldsort(ruleset.target.hosts);

// handle multiple TLDs rulesets
const domainGroups = splitDomainsByTlds(ruleset.target.hosts);
for (const domains of domainGroups) {
  // create temporary file
  const tmpFilename = path.join(
    os.tmpdir(),
    "FUXMERANDM-" + Date.now().toString()
  );

  fs.writeFileSync(tmpFilename, domains.join("\n"), {
    encoding: "utf8",
  });

  // construct commands from raw parts to rewrite the ruleset completely
  const rawCommandParts = [
    XHTTPSE_PATH,
    "--num-threads " + os.cpus().length,
    ruleset.securecookie ? "--securecookie" : null,
    "--name " +
      (domainGroups.length === 1
        ? `'${ruleset.metadata.name}'`
        : `'${domains[0]}'`),
    "-o " +
      (domainGroups.length === 1
        ? `'${filename}'`
        : `'${path.dirname(filename)}/${domains[0]}.xml'`),
    `'${tmpFilename}'`,
  ];

  const command = rawCommandParts.filter((part) => part != null).join(" ");
  child_process.execSync(command);

  // remove the original file if neccessary
  if (domainGroups.length > 1) {
    fs.unlinkSync(filename);
  }

  // remove temporary file
  fs.unlinkSync(tmpFilename);
}
