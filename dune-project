(lang dune 3.11)

; The above stanza specifies the Dune language version being used.

; Declares the use of the Melange compiler with a specific version.

(using melange 0.1)

; Declares the use of the directory-targets extension, useful for managing 
; directory-level build targets.

(using directory-targets 0.1)

; Sets the name of the project.

(name ahrefs_project)

; Indicates whether OPAM files should be automatically generated.

(generate_opam_files true)

; Provides the source location of the project, typically a GitHub repository.

(source
 (github Arrow7000/country-dropdown))

; Lists the authors of the project.

(authors "Aron Adler <aron@adler.dev>")

; Declares the license type under which the project is distributed.

(license MIT)

; Provides a link to the project's documentation.

(documentation https://github.com/Arrow7000/country-dropdown)

; The package stanza defines the configuration for an OPAM package.

(package
 (name ahrefs_project)
 (synopsis "Enter a synopsis of your app here")
 (description "Enter a description of your app here")
 ; Allows the package to be empty, which can be useful during development.
 (allow_empty)
 ; Lists the dependencies of the package along with version constraints.
 (depends
  (dune
   (>= 3.11))
  (melange
   (>= 2.1.0))
  (ocaml
   (>= 5.1.0))
  opam-check-npm-deps
  ppx_deriving
  (reason
   (>= 3.10.0))
  (reason-react
   (>= 0.13.0))
  reason-react-ppx
  melange-fetch
  melange-json
  melange-webapi)
 ; Tags to categorize or label the package, useful for searches and indexing.
 (tags
  (reasonml, ocaml, melange)))
