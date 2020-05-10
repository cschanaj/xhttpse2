"use strict";

/**
 * Sort domains alphabetically by their top-level-domains.
 * Remove duplicates if they exist.
 *
 * @param {string[]} domains An array of domains to be sorted
 * @returns {string[]} sorted domains
 */
function tldsort(domains) {
  return domains
    .map((domain) => domain.split(".").reverse().join("."))
    .sort()
    .map((domain) => domain.split(".").reverse().join("."))
    .filter((domain, index, arr) => arr.indexOf(domain, index + 1) < 0)
    .filter((domain) => domain.indexOf("*") === -1);
}

module.exports = tldsort;
