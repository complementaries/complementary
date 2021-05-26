#!/bin/bash

clang_format_command="clang-format"

cat <<EOF > .git/hooks/pre-commit
#!/bin/sh
FILES=\$(git diff --cached --name-only --diff-filter=ACMR "src/**/*.h" "src/**/*.cpp" "assets/shaders/**/*.vs" "assets/shaders/**/*.fs" "assets/shaders/**/*.gs" | sed 's| |\\ |g')
[ -z "\$FILES" ] && exit 0
echo "Applying auto formatting..."
# Format all changed files
echo "\$FILES" | xargs $clang_format_command -i -style=file
# Add back the modified files to staging
echo "\$FILES" | xargs git add
EOF

chmod +x .git/hooks/pre-commit

echo "Installed pre-commit hook."
